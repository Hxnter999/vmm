#pragma once
#include <commons.h>

union cr0_t {
	uint64_t value;
	struct {
		uint64_t pe : 1;            // pe: protection enabled	 r/w
		uint64_t mp : 1;            // mp: monitor coprocessor   r/w
		uint64_t em : 1;            // em: emulation             r/w
		uint64_t ts : 1;            // ts: task switched         r/w
		uint64_t et : 1;            // et: extension type        r
		uint64_t ne : 1;            // ne: numeric error         r/w
		uint64_t reserved6 : 10;
		uint64_t wp : 1;            // wp: write protect         r/w
		uint64_t reserved17 : 1;
		uint64_t am : 1;            // am: alignment mask        r/w
		uint64_t reserved19 : 10;
		uint64_t nw : 1;            // nw: not write-through     r/w
		uint64_t cd : 1;            // cd: cache disable         r/w
		uint64_t pg : 1;            // pg: paging                r/w
		uint64_t reserved32 : 32;
	};
};

union cr2_t {
	uint64_t value;
	struct {
		uint64_t pfl : 64;			// pfl: page-fault linear address
	};
};

union cr3_t {
	uint64_t value; // depends on cr4.pcide, if set, use pcide_on
	union {
		struct {
			uint64_t reserved0 : 3;
			uint64_t pwt : 1;           // pwt: page-level write-through
			uint64_t pcd : 1;           // pcd: page-level cache disable
			uint64_t reserved5 : 7;
			uint64_t pml4 : 40;         // pml4: page-map level-4 base address
			uint64_t reserved52 : 12;
		};
		struct {
			uint64_t pci : 12;			// pci: process-context identifier
			uint64_t pml4 : 40;
			uint64_t reserved52 : 12;
		}pcide_on;
	};
};

union cr4_t {
	uint64_t value;
	struct {
		uint64_t vme : 1;           // vme: setting this enables hardware-supported performance enhancements for software running in virtual - 8086 mode
		uint64_t pvi : 1;           // pvi: protected-mode virtual interrupts 
		uint64_t tsd : 1;           // tsd: time-stamp disable
		uint64_t de : 1;            // de: debugging extensions
		uint64_t pse : 1;           // pse: the pse bit is ignored when the processor is running in long mode
		uint64_t pae : 1;           // pae: long mode requires pae to be enabled
		uint64_t mce : 1;           // mce: setting this enables the machine-check exception mechanism.
		uint64_t pge : 1;           // pge: when page translation is enabled, system-software performance can often be improved by making some page translations global to all tasks and procedures.
		uint64_t pce : 1;           // pce: setting this allows software running at any privilege level to use the rdpmc instruction
		uint64_t osfxsr : 1;        // osfxsr: operating system fxsave/fxrstor support 
		uint64_t osxmmexcpt : 1;    // osxmmexcpt: operating system unmasked exception support 
		uint64_t umip : 1;          // umip: user mode instruction prevention 
		uint64_t la57 : 1;          // la57: 5-level paging enable 
		uint64_t reserved13 : 3;
		uint64_t fsgsbase : 1;      // fsgsbase: enable rdfsbase, rdgsbase, wrfsbase, and wrgsbase instructions
		uint64_t pcide : 1;         // pcide: process context identifier enable 
		uint64_t osxsave : 1;       // osxsave: xsave and processor extended states enable bit
		uint64_t reserved19 : 1;
		uint64_t smep : 1;          // smep: supervisor mode execution prevention
		uint64_t smap : 1;          // smap: supervisor mode access protection 
		uint64_t pke : 1;           // pke: enable support for memory protection keys. also enables support for the rdpkru and wrpkru instructions.
		uint64_t cet : 1;           // cet: control-flow enforcement technology
		uint64_t reserved24 : 40;
	};
};

union cr8_t {
	uint64_t value;
	struct {
		uint64_t tpr : 4;           // tpr: task-priority register
		uint64_t reserved4 : 60;
	};
};
