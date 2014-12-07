<?php

use Symfony\Component\HttpKernel\Kernel as BaseKernel;

/**
 * Class DrupalEmbeddedKernel
 */
abstract class DrupalEmbeddedKernel extends BaseKernel
{
    const EXTRA_VERSION   = 'drupal';

    /**
     * {@inheritdoc}
     *
     * @api
     */
    public function getCacheDir()
    {
        $cache_dir = 'public://var'.'/'.$this->getName().'/'.$this->environment.'/cache';
        file_prepare_directory($cache_dir, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);

        return drupal_realpath($cache_dir);
    }

    /**
     * {@inheritdoc}
     *
     * @api
     */
    public function getLogDir()
    {
        $log_dir = 'public://var'.'/'.$this->getName().'/'.$this->environment.'/logs';
        file_prepare_directory($log_dir, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);

        return drupal_realpath($log_dir);
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
