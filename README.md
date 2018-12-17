# Authorize helpers for Home Assistant user system

## Nginx-Frontend

```Dockefile
WORKDIR /usr/src

ARG HASSIO_AUTH_VERSION
RUN apk add --no-cache \
        nginx nginx-mod-http-lua \
    && git clone --depth 1 -b $HASSIO_AUTH_VERSION https://github.com/home-assistant/hassio-auth \
    && cd hassio-auth/nginx-frontend \
    && cp -f * /etc/nginx/
    && rm -r /usr/src/hassio-auth
```

```bash

```

## PAM

```Dockefile
WORKDIR /usr/src

ARG HASSIO_AUTH_VERSION
RUN apk add --no-cache \
        musl libcurl linux-pam \
    && apk add --no-cache --virtual .build-dependencies \
        musl-dev curl-dev linux-pam-dev gcc git make \
    && git clone --depth 1 -b $HASSIO_AUTH_VERSION https://github.com/home-assistant/hassio-auth \
    && cd hassio-auth/pam \
    && make \
    && cp -f pam_hassio.so /lib/security/ \
    && apk del .build-dependencies \
    && rm -r /usr/src/hassio-auth
```
