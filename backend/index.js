const http = require('http');
const { Client } = require('pg');
require('dotenv').config();

const hostname = '0.0.0.0';
const port = 3000;

const client = new Client({
    user: process.env.DB_USER,
    host: process.env.DB_HOST,
    database: process.env.DB_DATABASE,
    password: process.env.DB_PASSWORD,
    port: process.env.DB_PORT,
});

client.connect();

async function main(data) {
    const fecha = new Date().toLocaleString('en-US', 
        { 
            year: 'numeric', 
            month: '2-digit', 
            day: '2-digit', 
            hour: '2-digit', 
            minute: '2-digit', 
            second: '2-digit', 
            hour12: false 
        }
    );
    console.log(fecha);
    try{
        console.log(data);

        const sesionid = await client.query(
            'SELECT max(sesion.id) FROM sesion \
                JOIN usuario ON usuarioid = usuario.id \
                WHERE usuario.nombre LIKE $1', [data.nombre]
                );
        const sesion = sesionid.rows[0].max;

        //const result = await client.query('SELECT * FROM test');
        const result = await client.query(
            'INSERT INTO monitoreo (hora, intensidadled, oxigenacion, pulso, temperatura, sesionid) \
                VALUES ($1, $2, $3, $4, $5, $6)', 
                [fecha, data.intensidadled, data.oxigenacion, data.pulso, data.temperatura, sesion]
        );

        console.log(result.rows);
    } catch (error) {
        console.error("Encountered an error: ", error);
    }
}


const server = http.createServer((req, res) => {
    console.log("\nRecibiendo peticion...");
    if (req.method === 'POST' && req.headers['content-type'] === 'application/json') {
        let body = '';
        req.on('data', chunk => {
            body += chunk.toString(); // convert Buffer to string
        });
        req.on('end', () => {
            const data = JSON.parse(body); // parse the JSON data
            res.end('JSON received and printed');
            main(data);
        });
    } else {
        res.statusCode = 200;
        res.setHeader('Content-Type', 'text/plain');
        res.end('Hello, World!');
        console.log("Hello, World!");
    }
});

server.listen(port, hostname, () => {
    console.log(`Server running at http://${hostname}:${port}/`);
});

process.on('exit', () => {
    client.end();
    console.log("Client disconnected");
});