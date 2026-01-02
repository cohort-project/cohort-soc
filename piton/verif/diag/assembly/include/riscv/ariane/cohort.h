#include "cohort_fifo.h"
#include "cache_metrics.h"
#include "all_stats.h"

struct _cohort_t;
typedef struct _cohort_t cohort_t;

typedef uint8_t c_id_t;

typedef void (*print_perf_monitor_t)(void);
typedef void (*print_dbg_monitor_t)(void);
//typedef void (*turn_on_t)(void);
//typedef void (*turn_off_t)(void);
typedef void (*monitor_on_t)(void);
typedef void (*monitor_off_t)(void);
typedef void (*push_t)(uint64_t element, cohort_t* cohort);
typedef uint64_t (*pop_t)(cohort_t* cohort);



struct _cohort_t {
	uint8_t cohort_id; // which cohort is this in the system
	fifo_ctrl_t* sw_producer_fifo;
	fifo_ctrl_t* sw_consumer_fifo;
	
	push_t push;
	pop_t pop;

	void* custom_data;

    // Cache metrics
    uint64_t l2_access;
    uint64_t l2_misses;

//	turn_on_t turn_on;
//	turn_off_t turn_off;
//	print_perf_monitor_t print_perf_monitor;
//	print_dbg_monitor_t print_dbg_monitor;
//	monitor_on_t monitor_on;
//	monitor_off_t monitor_off;
};

cohort_t *cohort_init(c_id_t c_id, uint32_t fifo_length, uint16_t element_size);
void cohort_deinit(cohort_t* cohort);
void cohort_off(c_id_t c_id);
void cohort_on(c_id_t c_id);
void cohort_stop_monitors(c_id_t c_id);
void cohort_print_monitors(c_id_t c_id);
void cohort_print_debug_monitors(c_id_t c_id);
void cohort_collect_cache_metrics(cohort_t* cohort);
void cohort_print_cache_metrics(cohort_t* cohort);

cohort_t *cohort_init(c_id_t c_id, uint32_t fifo_length, uint16_t element_size)
{
	cohort_t  *cohort = (cohort_t *) malloc(sizeof(cohort_t));
	// an external software thread produces into this fifo
	cohort->sw_producer_fifo = fifo_init(fifo_length, element_size, 0, c_id);
	// an external software thread consumes from this fifo
	cohort->sw_consumer_fifo = fifo_init(fifo_length, element_size, 1, c_id);

	cohort->cohort_id = c_id;
//	cohort->print_perf_monitor = &cohort_print_monitors;
//	cohort->print_dbg_monitor = &cohort_print_debug_monitors;

    void *acc_address = memalign(128, 128);
    memset(acc_address, 0, 128);
	cohort_ni_write(c_id, 6, (uint64_t) acc_address);
	cohort->custom_data = acc_address;

	cohort_on(c_id);

	unsigned long long int write_value = 11;
    unsigned long long int serialization_value = 1;
    unsigned long long int deserialization_value = 1;
    unsigned long long int wait_counter = 0xa0;
    unsigned long long int backoff_counter = 0x100;

    write_value |= backoff_counter << 48;
    write_value |= serialization_value << 32;
    write_value |= deserialization_value << 16;
    write_value |= wait_counter << 4;
    __sync_synchronize;

    cohort_ni_write(c_id, 7, write_value);

	return cohort;
}

void cohort_deinit(cohort_t* cohort) 
{
	cohort_off(cohort->cohort_id);
	fifo_deinit(cohort->sw_consumer_fifo);
	fifo_deinit(cohort->sw_producer_fifo);
	free(cohort->custom_data);
	free(cohort);
}

//void cohort_push(uint64_t element, cohort_t* cohort) {
//	cohort->sw_producer_fifo->fifo_push_func(element, cohort->sw_producer_fifo);
//}
//
//void cohort_push_sync(cohort_t* cohort){
//	fifo_push_sync(cohort->sw_producer_fifo);
//}
//
//uint64_t cohort_pop(cohort_t* cohort) {
//	return cohort->sw_consumer_fifo->fifo_pop_func(cohort->sw_consumer_fifo);
//}
//
//void cohort_pop_sync(cohort_t* cohort) {
//	fifo_pop_sync(cohort->sw_consumer_fifo);
//}

void cohort_off(c_id_t c_id)
{
	cohort_stop_monitors(c_id);
#ifdef COHORT_DEBUG
	cohort_print_monitors(c_id);
	cohort_print_debug_monitors(c_id);
#endif
    cohort_ni_write(c_id, 7, 0);
    __sync_synchronize;
#ifndef BARE_METAL
    // don't flush in bare metal, because some things can go wrong
    dec_flush_tlb(0);
#endif
}

void cohort_on(c_id_t c_id)
{
    // turn on the monitor
    // don't lower reset, but turn on and clear the monitor
    cohort_ni_write(c_id, 7, 6);
    __sync_synchronize;
}

void cohort_stop_monitors(c_id_t c_id)
{
    // stop counter, but keep reset low
    cohort_ni_write(c_id, 7, 1);
}

void cohort_print_monitors(c_id_t c_id)
{
    for (int i=0;i< 35; i++) {
        printf("%lx,",cohort_ni_read(c_id, i));
    }

}

void cohort_print_debug_monitors(c_id_t c_id)
{
    printf("here's the debug registers dump\n");
    for (int i=35;i< 55; i++) {
        printf("dbg reg %d: %lx\n",i - 35, cohort_ni_read(c_id, i));
    }

}

void cohort_collect_cache_metrics(cohort_t* cohort) {
    uint8_t coreid = cohort->cohort_id;
    cohort->l2_access = read_L2_access(coreid);
    cohort->l2_misses = read_L2_misses(coreid);
}

void cohort_print_cache_metrics(cohort_t* cohort) {
    printf("Cohort ID: %d\n", cohort->cohort_id);
    printf("L2 Accesses: %ld\n", cohort->l2_access);
    printf("L2 Misses: %ld\n", cohort->l2_misses);
}

void cohort_execute_with_stats(c_id_t c_id, void (*code)(void), uint32_t iter) {
    all_stats(code, iter); // Use all_stats macro to collect and print performance metrics
}