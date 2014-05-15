Symfony Kernel
==============

Embed a Symfony Kernel in your Drupal application. When a site visitor requests a Symfony
route, the kernel handles the request inside a Drupal page callback.

Normal page delivery can be short circuited entirely so that the Symfony response
is always sent, but the normal arrangement is that HTML responses are embedded inside
Drupal pages as the `$content` variable in `page.tpl.php`. Non-HTML responses are sent
directly to the user.

Usage
-----
Implement `hook_kernel_info()` and name your Symfony kernel `app` (only one is supported).

The kernel class must be autoloadable (unlike in Symfony's front controllers which include
it). Put it in a module's info file.

```ini
name = My awesome Symfony application
core = 7.x
dependencies[] = kernel
files[] = app/AppKernel.php
```

The `app` and `src` directories in this module are a starting point for your own project.

Use `drush app` to use your Symfony kernel's console commands.

To do
-----
* Synchronize kernel events with Drupal bootstrap. See
  <https://github.com/bangpound/drupal-bundle/blob/master/EventListener/BootstrapListener.php>
* Cooperate with Drupal's session handler. See
  <https://github.com/bangpound/drupal-bundle/blob/master/DrupalSessionHandler.php>
* Expose Drupal users to Symfony with a UserProvider. See
  <https://github.com/bangpound/drupal-bundle/blob/master/Security/User/UserProvider.php>
* Support multiple kernels with [LazyHttpKernel](https://github.com/stackphp/LazyHttpKernel)
  and [UrlMap](https://github.com/stackphp/url-map).

Known issues
------------
* Symfony web profiler should be disabled in your kernel. It cannot offer an accurate
  picture of your requests and responses anyway!
* Assetic is hard to use. You can't use the Assetic controller, but you can dump assets
  and use them from Drupal. Documentation forthcoming.
  
Screenshots
-----------

![Drupal running the Symfony Acme Demo bundle](http://bangpound.org/kernel/drupal.png)

![Output of `drush app`](http://bangpound.org/kernel/drush.png)
