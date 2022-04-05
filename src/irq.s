.global disable_irqs

disable_irqs:
    msr    daifset, #2
    ret
