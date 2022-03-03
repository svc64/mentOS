.global disable_irqs
.global enable_irqs

disable_irqs:
    msr    daifset, #2
    ret

enable_irqs:
    msr    daifclr, #2
    ret
