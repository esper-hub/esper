The build-container allows to build sming in a defined environment.

Usage
=====
*All commands must be executed from this directory*

Build the docker container:
```shell
docker build --tag esper-build .
```
This must be done only once after updating the container.

Run the container manually:
```shell
docker run \
    --tty \
    --interactive \
    --device ${DEVICE}:/dev/ttyESP
    --volume ${ESPER}:/home/builder/esper
    --volume ${SITE}:/home/builder/site
    esper-build \
    make
```
Replace `${DEVICE}`, `${ESPER}` and `${SITE}` with the correct paths.

