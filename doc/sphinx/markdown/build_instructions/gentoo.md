# Installation for Gentoo users


## Overlay

Up to date ebuilds are found in our [sft overlay](https://github.com/SFTtech/gentoo-overlay/tree/master/games-strategy/openage).

For automatic updates, install the overlay with [layman](https://wiki.gentoo.org/wiki/Layman)!

``` shell
# add the sft-overlay
layman -a sft
```

## Installation

``` shell
# install openage
emerge -avt openage
```

This will install the latest release.


### git version

If you want to install the latest git version:

```
# In /etc/portage/package.keywords, add
=games-strategy/openage-9999::sft **
```

Beware, the git version does **not** do automatic updates.
You have to explicitly "reinstall" openage or use `app-portage/smart-live-rebuild`.
