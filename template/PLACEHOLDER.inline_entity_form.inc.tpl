<?php print $a->php(); ?>

class <?php echo $a->machineName(); ?>InlineEntityFormController extends EntityInlineEntityFormController  {
  const _entity_type = '<?php echo $a->machineName(); ?>';

  public function __construct(array $settings = []) {
    parent::__construct($this::_entity_type, $settings);
  }
}
