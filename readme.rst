=============
MQTT for ABMT
=============

Dependencies
============
- libmosquitto-dev



mqtt2sql
========
Setup Postgres
---------------

`apt install postgresql libpqxx-dev`

`
su postgres -c psql
postgres=# create database mqtt_db;
postgres=# create user mqtt_user with encrypted password 'mqtt_passwd';
postgres=# grant all privileges on database mqtt_db to mqtt_user;
\connect mqtt_db

mqtt_db=# CREATE SEQUENCE IF NOT EXISTS public.json_topic_id_seq
    INCREMENT 1
    START 1
    MINVALUE 1
    MAXVALUE 2147483647
    CACHE 1;

mqtt_db=# CREATE TABLE IF NOT EXISTS public.json_topic
(
    id integer NOT NULL DEFAULT nextval('json_topic_id_seq'::regclass),
    topic character varying(255) COLLATE pg_catalog."default",
    date timestamp without time zone,
    data json,
    CONSTRAINT json_topic_pkey PRIMARY KEY (id)
);

mqtt_db=# ALTER SEQUENCE public.json_topic_id_seq OWNER TO mqtt_user;
mqtt_db=# ALTER TABLE IF EXISTS public.json_topic OWNER to mqtt_user;
`

Setup Mosqitto
--------------
`apt install mosquitto libmosquitto-dev`

/etc/mosquitto/conf.d/local.conf:

`listener 1883
allow_anonymous true`

Build
-----

`systemctl enable /home/pi/ws/mqtt/mqtt2sql/mqtt2sql.service`
