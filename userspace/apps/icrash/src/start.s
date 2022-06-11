.global start
start:
    msr hcr_el2, x0
    b start
