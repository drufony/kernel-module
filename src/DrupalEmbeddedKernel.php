<?php

use Symfony\Component\HttpKernel\Kernel as BaseKernel;

/**
 * Class DrupalEmbeddedKernel
 */
abstract class DrupalEmbeddedKernel extends BaseKernel
{
    /**
     * {@inheritdoc}
     *
     * @api
     */
    public function getCacheDir()
    {
        $cache_dir = drupal_realpath('public://var/' .$this->getName() .'/'. $this->environment .'/cache');
        file_prepare_directory($cache_dir, FILE_CREATE_DIRECTORY);

        return $cache_dir;
    }

    /**
     * {@inheritdoc}
     *
     * @api
     */
    public function getLogDir()
    {
        $log_dir = drupal_realpath('public://var/' .$this->getName() .'/'. $this->environment .'/logs');
        file_prepare_directory($log_dir, FILE_CREATE_DIRECTORY);

        return $log_dir;
    }

    /**
     * Returns the kernel parameters.
     *
     * @return array An array of kernel parameters
     */
    protected function getKernelParameters()
    {
        $parameters = parent::getKernelParameters();
        $parameters['kernel.drupal_root'] = DRUPAL_ROOT;
        $parameters['kernel.conf_path'] = conf_path();

        return $parameters;
    }
}
