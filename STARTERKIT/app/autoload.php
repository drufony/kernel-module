<?php

use Doctrine\Common\Annotations\AnnotationRegistry;
use Composer\Autoload\ClassLoader;

/**
 * @var ClassLoader $loader
 */
$loader = ComposerAutoloaderInitComposerManager::getLoader();

AnnotationRegistry::registerLoader(array($loader, 'loadClass'));

return $loader;
