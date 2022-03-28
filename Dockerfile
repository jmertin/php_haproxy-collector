FROM php:7.4-fpm-alpine
MAINTAINER Joerg Mertin <joerg.mertin@broadcom.com>

# Add hapcoll user
RUN adduser -h /opt/hapcoll -s /sbin/nologin -g "HAProxy collector" -D hapcoll

# Updating alpine image, adding nano editor
RUN apk upgrade --no-cache && docker-php-ext-install pcntl
# Replace previous file with this one for troubleshooting
#RUN apk upgrade --no-cache && apk add nano busybox-extras strace bash ca-certificates && docker-php-ext-install pcntl

# Install haproxy collector code
RUN mkdir /opt/hapcoll/conf
COPY conf/* /opt/hapcoll/conf/
COPY hacollector.php /opt/hapcoll/hacollector.php
COPY run.sh /opt/hapcoll/run.sh
RUN chown hapcoll.hapcoll -R /opt/hapcoll

USER hapcoll
WORKDIR /opt/hapcoll
CMD [ "php", "/opt/hapcoll/hacollector.php" ]

