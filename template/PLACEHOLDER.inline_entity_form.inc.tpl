<?php print $a['php'] ?>

class <?php echo $a['machine_camel'] ?>InlineEntityFormController extends EntityInlineEntityFormController  {
  const _entity_type = "<?php echo $a['type_const'] ?>";

  public function __construct(array $settings = []) {
    parent::__construct($this::_entity_type, $settings);
  }
}
