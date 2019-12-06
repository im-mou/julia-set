movapd %xmm0,%xmm3
movapd %xmm2,%xmm1
mulsd  %xmm0,%xmm3
mulsd  %xmm2,%xmm1
movapd %xmm3,%xmm4
addsd  %xmm1,%xmm4
comisd %xmm4,%xmm7
ja     .L1
