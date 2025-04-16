### Running with Docker

Docker simplifies the setup process by providing a consistent development environment. It allows you to build and run the project without manually installing dependencies. Currently provided [Dockerfile.ubuntu.2404](../../packaging/docker/devenv/Dockerfile.ubuntu.2404) supports Ubuntu 24.04 as the base operating system.

#### Prerequisites

- **Docker**: Ensure Docker is installed on your machine. Follow the [official documentation](https://docs.docker.com/) for installation instructions.
- **Display Server**: This guide supports both **X11** and **Wayland** display servers for GUI applications.
- **Tested Configuration**: These instructions were tested on Ubuntu 24.04 running on WSL2 on Windows 11.

#### Steps to Build and Run

1. **Enable GUI Support**

   Depending on your display server, follow the appropriate steps:

   - **For X11**:
     Allow the Docker container to access your X server:
     ```bash
     xhost +local:root
     ```

   - **For Wayland**:
     Allow the Docker container to access your Wayland socket:
     ```bash
     sudo chmod a+rw /run/user/$(id -u)/wayland-0
     ```

2. **Build the Docker Image**

   Build the Docker image using the provided Dockerfile:
   ```bash
   sudo docker build -t openage -f packaging/docker/devenv/Dockerfile.ubuntu.2404 .
   ```

3. **Run the Docker Container**

   Start the Docker container with the appropriate configuration for your display server:

   - **For X11**:
     ```bash
     docker run -it \
       -e DISPLAY=$DISPLAY \
       -v /tmp/.X11-unix:/tmp/.X11-unix \
       -v $HOME/.Xauthority:/root/.Xauthority \
       --network host openage
     ```

   - **For Wayland**:
     ```bash
     docker run -it \
       -e XDG_RUNTIME_DIR=/tmp \
       -e QT_QPA_PLATFORM=wayland \
       -e WAYLAND_DISPLAY=$WAYLAND_DISPLAY \
       -v $XDG_RUNTIME_DIR/$WAYLAND_DISPLAY:/tmp/$WAYLAND_DISPLAY \
       --user=$(id -u):$(id -g) \
       --network host openage
     ```

4. **Follow the Regular Setup**

Once inside the container, follow the regular setup described in the [Development](../building.md#development) chapter. You can skip dependency installation since the Docker image already includes all required dependencies.

#### Notes

- **X11 vs. Wayland**: Ensure you know which display server your system is using. Most modern Linux distributions default to Wayland, but X11 is still widely used.
- **Permissions**: For Wayland, you may need to adjust permissions for the Wayland socket (`/run/user/$(id -u)/wayland-0`) to allow Docker access.
- **GUI Applications**: These configurations enable GUI applications to run inside the Docker container.

By following these steps, you can build and run the `openage` project in a Dockerized environment with support X11 or Wayland display servers.
