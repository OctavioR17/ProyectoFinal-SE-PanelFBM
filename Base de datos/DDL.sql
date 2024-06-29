create table tratamiento(
	id INT primary key generated always as identity,
	hora timestamp not null,
	tratamiento VARCHAR(150) not null,
	intensidad float not null,
	frecuencia float not null,
	oxigenacion float not null,
	pulso float not null,
	temperatura float not null
);