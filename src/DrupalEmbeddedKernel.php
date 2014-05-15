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
        return conf_path().'/'. $this->getName() .'/'. $this->environment .'/cache';
    }

    /**
     * {@inheritdoc}
     *
     * @api
     */
    public function getLogDir()
    {
        return conf_path().'/'. $this->getName() .'/'. $this->environment .'/logs';
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
