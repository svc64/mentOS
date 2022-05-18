// Granularity
#define PT_PAGE     0b11        // Table on levels other than the last
#define PT_BLOCK    0b01        // Block (not permitted on the last level)
// Accessibility
#define PT_KERNEL   (0<<6)      // Kernel memory
#define PT_USER     (1<<6)      // User memory
#define PT_RW       (0<<7)      // R/W
#define PT_RO       (1<<7)      // R/O
#define PT_AF       (1<<10)     // Accessed flag
#define PT_NX       (1UL<<54)   // No execute
// Shareability
#define PT_OSH      (2<<8)      // Outer shareable
#define PT_ISH      (3<<8)      // Inner shareable
// Defined in MAIR register
#define PT_MEM      (0<<2)      // Normal memory
#define PT_DEV      (1<<2)      // Device MMIO
#define PT_NC       (2<<2)      // Non-cachable

#define TTBR_CNP    1

#define TABLE_POINTER_ATTRIBS (PT_PAGE | PT_AF | PT_USER | PT_ISH | PT_MEM)

// id_aa64mmfr0_el1 PARange (bits 0-3) values
#define PARange_32 0b0000LL
#define PARange_36 0b0001LL
#define PARange_40 0b0010LL
#define PARange_42 0b0011LL
#define PARange_44 0b0100LL
#define PARange_48 0b0101LL

// Page sizes
#define SIZE_4K 4096
#define SIZE_16K 16384
#define SIZE_64K 65536

// MAIR attributes
#define MAIR_ATTR_DEVICE_nGnRnE		0x00UL
#define MAIR_ATTR_DEVICE_nGnRE		0x04UL
#define MAIR_ATTR_NORMAL_NC		    0x44UL
#define MAIR_ATTR_NORMAL_TAGGED		0xf0UL
#define MAIR_ATTR_NORMAL		    0xffUL
#define MAIR_ATTR_MASK			    0xffUL
// Macro to write the attributes to the correct index
#define MAIR_ATTRIDX(attr, idx)		((attr) << ((idx) * 8))
// TCR register flags
#define TCR_T0SZ_OFFSET		0
#define TCR_T1SZ_OFFSET		16
#define TCR_T0SZ(x)		((64UL - (x)) << TCR_T0SZ_OFFSET)
#define TCR_T1SZ(x)		((64UL - (x)) << TCR_T1SZ_OFFSET)
#define TCR_TxSZ(x)		(TCR_T0SZ(x) | TCR_T1SZ(x))
#define TCR_TxSZ_WIDTH		6
#define TCR_T0SZ_MASK		(((1UL << TCR_TxSZ_WIDTH) - 1) << TCR_T0SZ_OFFSET)
#define TCR_T1SZ_MASK		(((1UL << TCR_TxSZ_WIDTH) - 1) << TCR_T1SZ_OFFSET)

#define TCR_EPD0_SHIFT		7
#define TCR_EPD0_MASK		(1UL << TCR_EPD0_SHIFT)
#define TCR_IRGN0_SHIFT		8
#define TCR_IRGN0_MASK		(3UL << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_NC		(0UL << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_WBWA		(1UL << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_WT		(2UL << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_WBnWA		(3UL << TCR_IRGN0_SHIFT)

#define TCR_EPD1_SHIFT		23
#define TCR_EPD1_MASK		(1UL << TCR_EPD1_SHIFT)
#define TCR_IRGN1_SHIFT		24
#define TCR_IRGN1_MASK		(3UL << TCR_IRGN1_SHIFT)
#define TCR_IRGN1_NC		(0UL << TCR_IRGN1_SHIFT)
#define TCR_IRGN1_WBWA		(1UL << TCR_IRGN1_SHIFT)
#define TCR_IRGN1_WT		(2UL << TCR_IRGN1_SHIFT)
#define TCR_IRGN1_WBnWA		(3UL << TCR_IRGN1_SHIFT)

#define TCR_IRGN_NC		(TCR_IRGN0_NC | TCR_IRGN1_NC)
#define TCR_IRGN_WBWA		(TCR_IRGN0_WBWA | TCR_IRGN1_WBWA)
#define TCR_IRGN_WT		(TCR_IRGN0_WT | TCR_IRGN1_WT)
#define TCR_IRGN_WBnWA		(TCR_IRGN0_WBnWA | TCR_IRGN1_WBnWA)
#define TCR_IRGN_MASK		(TCR_IRGN0_MASK | TCR_IRGN1_MASK)

#define TCR_ORGN0_SHIFT		10
#define TCR_ORGN0_MASK		(3UL << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_NC		(0UL << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_WBWA		(1UL << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_WT		(2UL << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_WBnWA		(3UL << TCR_ORGN0_SHIFT)

#define TCR_ORGN1_SHIFT		26
#define TCR_ORGN1_MASK		(3UL << TCR_ORGN1_SHIFT)
#define TCR_ORGN1_NC		(0UL << TCR_ORGN1_SHIFT)
#define TCR_ORGN1_WBWA		(1UL << TCR_ORGN1_SHIFT)
#define TCR_ORGN1_WT		(2UL << TCR_ORGN1_SHIFT)
#define TCR_ORGN1_WBnWA		(3UL << TCR_ORGN1_SHIFT)

#define TCR_ORGN_NC		(TCR_ORGN0_NC | TCR_ORGN1_NC)
#define TCR_ORGN_WBWA		(TCR_ORGN0_WBWA | TCR_ORGN1_WBWA)
#define TCR_ORGN_WT		(TCR_ORGN0_WT | TCR_ORGN1_WT)
#define TCR_ORGN_WBnWA		(TCR_ORGN0_WBnWA | TCR_ORGN1_WBnWA)
#define TCR_ORGN_MASK		(TCR_ORGN0_MASK | TCR_ORGN1_MASK)

#define TCR_SH0_SHIFT		12
#define TCR_SH0_MASK		(3UL << TCR_SH0_SHIFT)
#define TCR_SH0_INNER		(3UL << TCR_SH0_SHIFT)

#define TCR_SH1_SHIFT		28
#define TCR_SH1_MASK		(3UL << TCR_SH1_SHIFT)
#define TCR_SH1_INNER		(3UL << TCR_SH1_SHIFT)
#define TCR_SHARED		(TCR_SH0_INNER | TCR_SH1_INNER)

#define TCR_TG0_SHIFT		14
#define TCR_TG0_MASK		(3UL << TCR_TG0_SHIFT)
#define TCR_TG0_4K		(0UL << TCR_TG0_SHIFT)
#define TCR_TG0_64K		(1UL << TCR_TG0_SHIFT)
#define TCR_TG0_16K		(2UL << TCR_TG0_SHIFT)

#define TCR_TG1_SHIFT		30
#define TCR_TG1_MASK		(3UL << TCR_TG1_SHIFT)
#define TCR_TG1_16K		(1UL << TCR_TG1_SHIFT)
#define TCR_TG1_4K		(2UL << TCR_TG1_SHIFT)
#define TCR_TG1_64K		(3UL << TCR_TG1_SHIFT)

#define TCR_IPS_SHIFT		32
#define TCR_IPS(x)      ((x) << TCR_IPS_SHIFT)
#define TCR_IPS_MASK	(7UL << TCR_IPS_SHIFT)
#define TCR_A1			(1UL << 22)
#define TCR_ASID16		(1UL << 36)
#define TCR_TBI0		(1UL << 37)
#define TCR_TBI1		(1UL << 38)
#define TCR_HA			(1UL << 39)
#define TCR_HD			(1UL << 40)
#define TCR_TBID1		(1UL << 52)
#define TCR_NFD0		(1UL << 53)
#define TCR_NFD1		(1UL << 54)
#define TCR_E0PD0		(1UL << 55)
#define TCR_E0PD1		(1UL << 56)
#define TCR_TCMA0		(1UL << 57)
#define TCR_TCMA1		(1UL << 58)
