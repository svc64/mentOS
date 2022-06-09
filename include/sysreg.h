#define BIT(x) (1 << (x))
// SCTLR_ELx flags
#define SCTLR_ELx_DSSBS	(BIT(44))
#define SCTLR_ELx_ATA	(BIT(43))

#define SCTLR_ELx_TCF_SHIFT	40
#define SCTLR_ELx_TCF_NONE	(0UL << SCTLR_ELx_TCF_SHIFT)
#define SCTLR_ELx_TCF_SYNC	(1UL << SCTLR_ELx_TCF_SHIFT)
#define SCTLR_ELx_TCF_ASYNC	(2UL << SCTLR_ELx_TCF_SHIFT)
#define SCTLR_ELx_TCF_ASYMM	(3UL << SCTLR_ELx_TCF_SHIFT)
#define SCTLR_ELx_TCF_MASK	(3UL << SCTLR_ELx_TCF_SHIFT)

#define SCTLR_ELx_ENIA_SHIFT	31

#define SCTLR_ELx_ITFSB	(BIT(37))
#define SCTLR_ELx_ENIA	(BIT(SCTLR_ELx_ENIA_SHIFT))
#define SCTLR_ELx_ENIB	(BIT(30))
#define SCTLR_ELx_ENDA	(BIT(27))
#define SCTLR_ELx_EE    (BIT(25))
#define SCTLR_ELx_IESB	(BIT(21))
#define SCTLR_ELx_WXN	(BIT(19))
#define SCTLR_ELx_ENDB	(BIT(13))
#define SCTLR_ELx_I	    (BIT(12))
#define SCTLR_ELx_SA	(BIT(3))
#define SCTLR_ELx_C	    (BIT(2))
#define SCTLR_ELx_A	    (BIT(1))
#define SCTLR_ELx_M	    (BIT(0))

#define ESR_ELx_EC_SHIFT            26
#define ESR_ELx_EC_MASK             (0b111111UL << ESR_ELx_EC_SHIFT)
#define ESR_ELx_EC(ecr)             (((ecr) & ESR_ELx_EC_MASK) >> ESR_ELx_EC_SHIFT)

#define ESR_EL1_EC_UNKNOWN          0b000000
#define ESR_EL1_EC_WFI_WFE          0b000001 // Trapped WFI or WFE instruction execution
#define ESR_EL1_EC_ENFP             0b000111 // Access to SVE, Advanced SIMD, or floating-point functionality
#define ESR_EL1_EC_ILLEGAL_EXEC     0b001110 // Illegal Execution state
#define ESR_EL1_EC_SVC_32           0b010001 // SVC instruction execution in AArch32 state
#define ESR_EL1_EC_SVC_64           0b010101 // SVC instruction execution in AArch64 state
#define ESR_EL1_EC_MRS_MSR_64       0b011000 // Trapped MSR, MRS or System instruction execution in AArch64 state
#define ESR_EL1_EC_IABT_LEL         0b100000 // Instruction Abort from a lower Exception level)
#define ESR_EL1_EC_IABT_CEL         0b100001 // Instruction Abort from a current Exception level
#define ESR_EL1_EC_PC_ALIGN         0b100010 // PC alignment fault exception
#define ESR_EL1_EC_DABT_LEL         0b100100 // Data Abort from a lower Exception level
#define ESR_EL1_EC_DABT_CEL         0b100101 // Data Abort from a current Exception level
#define ESR_EL1_EC_SP_ALIGN         0b100110 // SP alignment fault exception
#define ESR_EL1_EC_FP_32            0b101000 // Trapped floating-point exception taken from AArch32 state
#define ESR_EL1_EC_FP_64            0b101100 // Trapped floating-point exception taken from AArch64 state
#define ESR_EL1_EC_SError           0b101111 // SError
#define ESR_EC_BRK_DBG_EL0          0b110000 // Breakpoint exception from a lower Exception level
#define ESR_EC_BRK_DBG_EL1          0b110001 // Breakpoint exception taken without a change in Exception level
#define ESR_EC_SS_DBG_EL0           0b110010 // Software Step exception from a lower Exception level
#define ESR_EC_SS_DBG_EL1           0b110011 // Software Step exception taken without a change in Exception level
#define ESR_EC_WATCHP_EL0           0b110100 // Watchpoint exception from a lower Exception level
#define ESR_EC_WATCHP_EL1           0b110101 // Watchpoint exception taken without a change in Exception level
#define ESR_EC_BRK32                0b111000 // BKPT instruction execution in AArch32 state
#define ESR_EC_BRK64                0b111100 // BRK instruction execution in AArch64 state
