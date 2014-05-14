<?php

use Doctrine\Common\Annotations\AnnotationRegistry;
use Composer\Autoload\ClassLoader;

/**
 * @var ClassLoader $loader
 */
$loader = $GLOBALS['loader'];

AnnotationRegistry::registerLoader(array($loader, 'loadClass'));

return $loader;
