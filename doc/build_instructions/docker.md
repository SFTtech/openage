# Building with Docker

The [development images](../../packaging/docker/devenv/) contain openage's build dependencies.
From the repository root, build an image and open a shell with the checkout mounted:

```bash
docker build -t openage-devenv -f packaging/docker/devenv/ubuntu.dockerfile .

docker run --rm -it \
  --user "$(id -u):$(id -g)" \
  --device /dev/dri \
  --group-add "$(getent group video | cut -d: -f3)" \
  --group-add "$(getent group render | cut -d: -f3)" \
  --env HOME=/tmp \
  --mount "type=bind,src=$PWD,dst=/openage" \
  --workdir /openage \
  openage-devenv bash
```

Running as your host UID and GID keeps build output owned by you.
The group lookups pass the host's numeric `video` and `render` GIDs, regardless of the image's group definitions.
If your GPU devices use different groups, substitute the GIDs shown by `ls -ln /dev/dri`.
Inside the container:

```bash
./configure --compiler=g++-16 --download-nyan
just build
just tests
```

Use a separate checkout for container builds: host build directories and Python environments aren't portable into the image.
See [building](../building.md) for build options and [running](../running.md) for game assets and launch commands.

## Display access

The following options are for a local Linux desktop using Docker without user-namespace remapping.
Add the options for your display server to `docker run`, before `openage-devenv bash`.
Neither case requires changing host socket permissions or granting access with `xhost`.

### X11 or Xwayland

Pass the display socket and your existing authentication cookie:

```bash
--env DISPLAY \
--env QT_QPA_PLATFORM=xcb \
--env XAUTHORITY=/tmp/openage-xauthority \
--hostname "$(hostname)" \
--mount type=bind,src=/tmp/.X11-unix,dst=/tmp/.X11-unix,readonly \
--mount "type=bind,src=${XAUTHORITY:-$HOME/.Xauthority},dst=/tmp/openage-xauthority,readonly"
```

The authority file must exist and contain the cookie for `$DISPLAY`.
Matching the host name lets X11 find local-display cookies inside the container.

### Wayland

Mount only the compositor socket into a private runtime directory:

```bash
--env XDG_RUNTIME_DIR=/run/openage \
--env WAYLAND_DISPLAY=wayland-0 \
--env QT_QPA_PLATFORM=wayland \
--tmpfs "/run/openage:uid=$(id -u),gid=$(id -g),mode=0700" \
--mount "type=bind,src=${XDG_RUNTIME_DIR:?}/${WAYLAND_DISPLAY:-wayland-0},dst=/run/openage/wayland-0,readonly"
```

Native Wayland also needs Qt's Wayland platform plugin in the image (`qt6-wayland` on Ubuntu).
Alternatively, use Xwayland with the X11 options above.

### GPU access

The container command passes `/dev/dri` for GPU access.
NVIDIA setups also need the NVIDIA Container Toolkit.
