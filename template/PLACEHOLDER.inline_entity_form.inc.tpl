<?php print $a->php->value; ?>

class <?php echo $a->uc; ?>InlineEntityFormController extends EntityInlineEntityFormController  {
  const _entity_type = '<?php echo $a->uc; ?>';

  public function __construct(array $settings = []) {
    parent::__construct(self::_entity_type, $settings);
  }
}
