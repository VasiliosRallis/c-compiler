.set noreorder
	.text
	.global main
main:
addiu $sp, $sp, -12
sw $fp, 8($sp)
sw $31, 4($sp)
move $fp, $sp
li $v0, 2
move $sp, $fp
lw $31, 4($fp)
lw $fp, 8($sp)
addiu $sp, $sp, +12
jr $31
nop
