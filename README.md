# Authorize helpers for Home Assistant user system

## Nginx-Frontend

```Dockefile
WORKDIR /usr/src

ARG HASSIO_AUTH_VERSION
RUN apk add --no-cache \
        nginx nginx-mod-http-lua lua-resty-http \
    && git clone --depth 1 -b $HASSIO_AUTH_VERSION https://github.com/home-assistant/hassio-auth \
    && cd hassio-auth/nginx-frontend \
    && cp -f * /etc/nginx/ \
    && rm -r /usr/src/hassio-auth
```

```bash
WAIT_PIDS=()
PORT=8431

if [ "$SSL" == "true" ];
    sed -i "s/%%PORT%%/${PORT}/g" /etc/nginx/nginx-ssl.conf
    sed -i "s/%%CERTFILE%%/${CERTFILE}/g" /etc/nginx/nginx-ssl.conf
    sed -i "s/%%KEYFILE%%/${KEYFILE}/g" /etc/nginx/nginx-ssl.conf

    nginx -c /etc/nginx/nginx-ssl.conf &
else
    sed -i "s/%%PORT%%/${PORT}/g" /etc/nginx/nginx.conf

    nginx -c /etc/nginx/nginx.conf &
fi
WAIT_PIDS+=($!)

# Register stop
function stop_addon() {
    echo "Kill Processes..."
    kill -15 "${WAIT_PIDS[@]}"
    wait "${WAIT_PIDS[@]}"
    echo "Done."
}
trap "stop_addon" SIGTERM SIGHUP

# Wait until all is done
wait "${WAIT_PIDS[@]}"
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
