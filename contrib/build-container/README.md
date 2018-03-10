The build-container allows to build sming in a defined environment.

It is available at `esperhub/esper` with tags `latest` (master branch) and `develop`.

# Usage
*All commands must be executed from this directory*

## Build the docker container (optional):
```shell
docker build --tag esper-build .
```
This must be done only once after updating the container.

## Run the docker container

```shell
docker run \
    --tty \
    --interactive \
    --device ${DEVICE}:/dev/ttyESP
    --volume ${ESPER}:/home/builder/esper
    --volume ${SITE}:/home/builder/site
    esperhub:esper
```
Replace `${DEVICE}`, `${ESPER}` and `${SITE}` with the correct paths.

