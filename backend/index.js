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

function getFecha() {
    let fecha = new Date();
    return fecha.getFullYear() + "/" +
    (fecha.getMonth() + 1) + "/" +
    fecha.getDate() + " " +
    fecha.getHours() + ":" +
    fecha.getMinutes() + ":" +
    fecha.getSeconds()
}

async function insertar(data) {
    const hora = getFecha();
    
    console.log(hora);
    console.log(data);
    try{
        console.log("Insertando datos...");

        const result = await client.query(
            'INSERT INTO tratamiento (hora, tratamiento, intensidad, frecuencia, oxigenacion, pulso, temperatura) \
                VALUES ($1, $2, $3, $4, $5, $6, $7)', 
                [hora, data.tratamiento, data.intensidad, data.frecuencia, data.oxigenacion, data.pulso, data.temperatura]
        );
        console.log("Datos insertados!");
    } catch (error) {
        console.error("Encountered an error: ", error);
    }
}

async function consultar(){
    try {
        const query = 'SELECT * FROM tratamiento'; // Replace with your actual query
        const res = await client.query(query);
        return res.rows; // Assuming you want to return the rows from the query
      } catch (err) {
        console.error(err);
        throw err; // Or handle the error as needed
      }
}

const server = http.createServer((req, res) => {

    // Set CORS headers
    res.setHeader('Access-Control-Allow-Origin', '*'); // Change '*' to your frontend's origin for security
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

    // Handle preflight requests
    if (req.method === 'OPTIONS') {
        res.statusCode = 204; // No Content
        res.end();
        return;
    }

    console.log("\nRecibiendo peticion...");
    if (req.method === 'POST' && req.headers['content-type'] === 'application/json') {
        let body = '';
        req.on('data', chunk => {
            body += chunk.toString(); // convert Buffer to string
        });
        req.on('end', () => {
            const data = JSON.parse(body); // parse the JSON data
            res.end('JSON received and printed');
            insertar(data);
        });
    } else if (req.method === 'GET') {

        consultar().then( result => {
            const datos = result;

            res.statusCode = 200;
            res.setHeader('Content-Type', 'application/json');
            res.end(JSON.stringify(datos));
            console.log("Data sent!");
        })
    } else {
        res.statusCode = 404;
        res.end('Not Found');
        console.log("Not Found");
    }
});

server.listen(port, hostname, () => {
    console.log(`Server running at http://${hostname}:${port}/`);
});

process.on('exit', () => {
    client.end();
    console.log("Client disconnected");
});