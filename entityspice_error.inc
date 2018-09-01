<?php

const _ENTITYSPICE_ERR_NO_ERR = 0;
const _ENTITYSPICE_ERR_MACHINE = 2;
const _ENTITYSPICE_ERR_INVALID = 4;
const _ENTITYSPICE_ERR_EMPTY = 8;
const _ENTITYSPICE_ERR_TOO_LONG = 16;
const _ENTITYSPICE_ERR_ALPHANUM = 32;
const _ENTITYSPICE_ERR_LABEL = 64;
//const _ENTITYSPICE_ERR_URI = 128;
const _ENTITYSPICE_ERR_ASCII = 256;

const _ENTITYSPICE_OVEN_MAX_NAME_LEN = 64;

function _entityspice_error_msg($error_no) {
  switch ($error_no) {
    case _ENTITYSPICE_ERR_NO_ERR:
      $msg = 'no error';
      break;

    case _ENTITYSPICE_ERR_MACHINE:
      $msg = "must conform to machine name, it should only contain ";
      $msg .= "lowercase letters, numbers, underscore and begin with a letter, ";
      $msg .= "it's length should be less than 63 characters and it can not ";
      $msg .= "be empty.";
      break;

    case _ENTITYSPICE_ERR_ALPHANUM:
      $msg = 'must only contain letters, numbers and underscores.';
      break;

    case _ENTITYSPICE_ERR_EMPTY:
      $msg = 'can not be empty.';
      break;

    case _ENTITYSPICE_ERR_TOO_LONG:
      $msg = 'must be less than 63 characters.';
      break;

    case _ENTITYSPICE_ERR_LABEL:
      $msg = 'must only contain letters, numbers, underscore and ' .
        'whitespaces, and it can not begin with space.';
      break;

    case _ENTITYSPICE_ERR_INVALID:
      $msg = 'invalid';
      break;

    case _ENTITYSPICE_ERR_ASCII:
      $msg = 'can only contain the character set: ' .
        '[A-Za-z0-9_!@#-=+:;[]{}?<>/?]';
      break;

    //    case _ENTITYSPICE_ERR_URI:
    //      $msg = 'does not conform to URI (note that URI must not begin or end with a slash).';
    //      break;

    default:
      $msg = 'invalid value (unknown error code: ' . $error_no . ')';
      break;
  }
  return $msg;
}

function _entityspice_token_validate($token_name, $value) {
  $tokens = _entityspice_tokens();
  $token = $tokens[$token_name];
  switch ($token['validation_policy']) {
    case _ENTITYSPICE_MACHINE:
      if (empty($value) || !($value)) {
        return _ENTITYSPICE_ERR_EMPTY;
      }
      elseif (!is_string($value)) {
        return _ENTITYSPICE_ERR_INVALID;
      }
      elseif (strlen($value) > _ENTITYSPICE_OVEN_MAX_NAME_LEN) {
        return _ENTITYSPICE_ERR_TOO_LONG;
      }
      elseif (preg_match('#[^a-z0-9_]#', $value) !== 0) {
        // Must contain valid characters.
        return _ENTITYSPICE_ERR_MACHINE;
      }
      elseif (preg_match('#^[^a-z]#', $value) !== 0) {
        // Must not start with a digit or underscore.
        return _ENTITYSPICE_ERR_MACHINE;
      }
      break;

    case _ENTITYSPICE_OVEN_MACHINE_CAMEL:
      if (_entityspice_token_validate(_ENTITYSPICE_MACHINE,
        @strtolower($value))) {
        return _ENTITYSPICE_ERR_INVALID;
      }
      break;

    case _ENTITYSPICE_OVEN_MACHINE_UPPER:
      if (_entityspice_token_validate(_ENTITYSPICE_MACHINE,
        @strtolower($value))) {
        return _ENTITYSPICE_ERR_INVALID;
      }
      break;

    case _ENTITYSPICE_OVEN_LABEL:
      if (preg_match('#[^a-zA-Z0-9_ ]#', $value) !== 0) {
        // Must contain valid characters.
        return _ENTITYSPICE_ERR_LABEL;
      }
      elseif (preg_match('#^[ ]#', $value) !== 0) {
        // Must not start with space
        return _ENTITYSPICE_ERR_LABEL;
      }
      break;

    case _ENTITYSPICE_OVEN_TYPE_CONST:
      return _entityspice_token_validate(_ENTITYSPICE_MACHINE,
        @strtolower($value));

    //    case _ENTITYSPICE_OVEN_ADMIN_BUNDLE_URI:
    //    case _ENTITYSPICE_OVEN_CONTENT_DEFAULT_URI:
    //    case _ENTITYSPICE_OVEN_DELETE_FORM_REDIRECT_URI:
    //      break;

    case 'ascii':
      if (preg_match('/[^ A-Za-z0-9_\!@#\-\=\+\:;\[\]{}\?\<\>]/', $value) !== 0) {
        // Must contain valid characters.
        return _ENTITYSPICE_ERR_ASCII;
      }
      break;
  }

  return _ENTITYSPICE_ERR_NO_ERR;
}

function _entityspice_token_post_process($validation_policy, $value) {
  drush_log("v: $value, val: $validation_policy");
  switch ($validation_policy) {
    case 'boolean':
      // We only know how to post process strings.
      if (!is_string($value)) {
        return $value;
      }
      switch (strtolower($value)) {
        case 'y':
        case 'yes':
        case 'true':
        case 'on':
        case 'ok':
        case '1':
          return TRUE;
      }
      return FALSE;

    default:
      return $value;
  }
}

