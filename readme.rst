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
``apt install postgresql libpqxx-dev``

Example output of creating a user and a json_topic table:

:: 

    # su postgres -c psql
    could not change directory to "/home/pi/smart/ws/mqtt/mqtt2sql": Permission denied
    psql (15.8 (Raspbian 15.8-0+deb12u1))
    Type "help" for help.

    postgres=# create database mqtt_db;
    CREATE DATABASE
    postgres=# create user mqtt_user with encrypted password 'mqtt_passwd';
    CREATE ROLE
    postgres=# grant all privileges on database mqtt_db to mqtt_user;
    GRANT
    postgres=# \connect mqtt_db
    You are now connected to database "mqtt_db" as user "postgres".
    mqtt_db=# CREATE TABLE IF NOT EXISTS public.topic
    (
        id integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 CACHE 1 ),
        topic character varying(255) COLLATE pg_catalog."default",
        CONSTRAINT topic_pkey PRIMARY KEY (id)
    );
    CREATE TABLE
    mqtt_db=# ALTER TABLE IF EXISTS public.topic OWNER to mqtt_user;
    ALTER TABLE
    mqtt_db=# CREATE TABLE IF NOT EXISTS public.json_data
    (
        id integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 CACHE 1 ),
        topic integer NOT NULL,
        date timestamp without time zone,
        data jsonb,
        CONSTRAINT json_data_pkey PRIMARY KEY (id),
        CONSTRAINT topic_fkey FOREIGN KEY (topic)
            REFERENCES public.topic (id) MATCH SIMPLE
            ON UPDATE CASCADE
            ON DELETE CASCADE
            NOT VALID
    );
    CREATE TABLE
    mqtt_db=# ALTER TABLE IF EXISTS public.json_data OWNER to mqtt_user;
    ALTER TABLE
:: 

You may also want to create indexes for faster access.

:: 

    mqtt_db=# CREATE INDEX date_idx
        ON public.json_data USING btree
        (date DESC NULLS LAST)
        WITH (deduplicate_items=True)
        TABLESPACE pg_default;
    CREATE INDEX
    mqtt_db=# CREATE INDEX topic_idx
        ON public.json_data USING btree
        (topic)
        WITH (deduplicate_items=True)
        TABLESPACE pg_default;
    CREATE INDEX
    mqtt_db=# CREATE INDEX date_topic_idx
        ON public.json_data USING btree
        (topic ASC NULLS LAST, date DESC NULLS FIRST)
        WITH (deduplicate_items=True)
        TABLESPACE pg_default;
    CREATE INDEX

::

Setup Mosqitto
--------------
::

    apt install mosquitto libmosquitto-dev

    /etc/mosquitto/conf.d/local.conf:

    listener 1883
    allow_anonymous true
::

Build
-----
::

    scons
    systemctl enable /home/pi/ws/mqtt/mqtt2sql/mqtt2sql.service
::
