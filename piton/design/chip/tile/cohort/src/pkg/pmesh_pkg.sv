// SPDX-License-Identifier: BSD-3-Clause
//
// Copyright (c) 2022 Tianrui Wei
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the authors nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

package pmesh_pkg;
	`include "dcp.h"
    typedef  logic [      `MSG_TYPE_WIDTH-1:0]  req_type_t;
    typedef  logic [    `DCP_MSHRID_WIDTH-1:0]  mshrid_t;
    typedef  logic [      `DCP_PADDR_MASK]      paddr_t;
    typedef  logic [     `DCP_UNPARAM_2_0]      size_t;
    typedef  logic [`PACKET_HOME_ID_WIDTH-1:0]  homeid_t;
    typedef  logic [  `MSG_AMO_MASK_WIDTH-1:0]  write_mask_t;
    typedef  logic [      `DCP_UNPARAM_63_0]    data_t;
    typedef  logic [`DCP_NOC_RES_DATA_SIZE-1:0] resp_data_t;

    // input and output are defined in the context of a client to the bus
	typedef struct packed {
		logic        valid     ;
		req_type_t   req_type  ;
		mshrid_t     mshrid    ;
		paddr_t      address   ;
		// real size = size * 8 bytes
		size_t       size      ;
		homeid_t     homeid    ;
		write_mask_t write_mask;
		data_t       data_0    ;
		data_t       data_1    ;
	} pmesh_noc2_o_t;

	typedef struct packed {
		logic ready;
	} pmesh_noc2_i_t;

	typedef struct packed {
		logic valid;
		mshrid_t mshrid;
		resp_data_t resp_data;
	} pmesh_noc3_in_t;
	
endpackage : pmesh_pkg