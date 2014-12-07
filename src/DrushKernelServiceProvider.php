<?php

use Pimple\ServiceProviderInterface;
use Symfony\Bundle\FrameworkBundle\Console\Application;

class DrushKernelServiceProvider implements ServiceProviderInterface
{
    /**
     * Registers services on the given container.
     *
     * This method should only be used to configure services and parameters.
     * It should not get services.
     *
     * @param \Pimple\Container $pimple An Container instance
     */
    public function register(\Pimple\Container $pimple)
    {
        $pimple['console.application'] = function ($c) {
            $application = new Application($c['kernel']);
            $application->setAutoExit(false);

            return $application;
        };
    }
}
