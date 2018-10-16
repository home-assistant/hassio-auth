

```Dockefile
WORKDIR /usr/src

ARG HASSIO_PAM_VERSION
RUN apk add --no-cache \
        musl libcurl linux-pam \
    && apk add --no-cache --virtual .build-dependencies \
        musl-dev curl-dev linux-pam-dev gcc git make \
    && git clone --depth 1 -b $HASSIO_PAM_VERSION https://github.com/home-assistant/hassio-auth-pam \
    && cd hassio-auth-pam \
    && make \
    && cp -f pam_hassio.so /lib/security/ \
    && apk del .build-dependencies \
    && rm -r /usr/src/hassio-auth-pam
```
