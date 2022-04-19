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
