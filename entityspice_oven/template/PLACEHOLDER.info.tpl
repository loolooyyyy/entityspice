name = <?php echo $a['module_name'] ?>
description = <?php echo $a['module_description'] ?>
core = 7.x

package = <?php echo $a['module_package'] ?>

; Don't worry, this is a soft dependency, you can gradually implement the parts
; that you want and remove dependency on entityspice.
dependencies[] = entityspice

; Classes
files[] = <?php echo $a['machine_name'] ?>.entity.inc
<?php if($a['has_inline_entity_form']): ?>files[] = lib/<?php echo $a['machine_name'] ?>.inline_entity_form.inc<?php endif; ?>
