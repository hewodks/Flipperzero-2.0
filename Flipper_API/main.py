# Informacion IMPORTANTE para conectarte a la API modificar o ver debes
# conectarte a la ip de donde esta montada la BD en este caso mi laptop
# ejemplo http://192.168.1.111:8000 en cualquier dispositivo

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
from pymongo import MongoClient
from datetime import datetime
from fastapi.exceptions import RequestValidationError
from fastapi.responses import JSONResponse
import certifi

app = FastAPI(title="Flipper ESP32 Logger API")

# --- CONFIGURACIÓN DE MONGODB ---
MONGO_URI = "mongodb+srv://eduardosegura7109_db_user:fNHhj5hO9xhVX5YM@clusterpractica.eyh2fpx.mongodb.net/?appName=ClusterPractica"

try:
    # Agregamos una configuración SSL agresiva para desarrollo que ignora por completo la validación local de certificados
    client = MongoClient(
        MONGO_URI,
        ssl=True,
        tlsCAFile=certifi.where()
    )
    
    # Selecciona la base de datos y la colección
    db = client["Flipper_Zero"]
    logs_collection = db["Logs_API"]
    print("Instancia del cliente MongoDB configurada exitosamente.")
except Exception as e:
    print("Error al configurar el cliente de MongoDB:", e)

# --- MODELO DE DATOS (Pydantic) ---
class LogSchema(BaseModel):
    modulo: str
    funcion: str
    duracion_ms: int

# --- ENDPOINTS (Consultas API) ---

@app.get("/")
def inicio():
    return {"mensaje": "API del Flipper-ESP32 activa y funcionando"}

# 1. Endpoint para que el ESP32 guarde los logs (POST)
@app.post("/api/logs")
def crear_log(log: LogSchema):
    try:
        # Usamos model_dump() que es la forma correcta en versiones actuales de Pydantic
        nuevo_log = log.model_dump()
        
        # Agregamos la fecha y hora exacta del servidor en formato ISO
        nuevo_log["timestamp"] = datetime.now().isoformat()
        
        # Intentamos la inserción real en MongoDB Atlas
        resultado = logs_collection.insert_one(nuevo_log)
        
        return {
            "estado": "exitoso", 
            "mensaje": "Log guardado correctamente", 
            "id_insertado": str(resultado.inserted_id)
        }
    except Exception as e:
        print("¡ERROR CRÍTICO AL INSERTAR EN MONGO! ->", str(e))
        raise HTTPException(status_code=500, detail=f"Error al guardar en la base de datos: {str(e)}")

# Este bloque atrapará cualquier error de formato que mande el cliente
@app.exception_handler(RequestValidationError)
async def validation_exception_handler(request, exc):
    print("¡ERROR DE VALIDACIÓN DETECTADO! ->", str(exc))
    return JSONResponse(
        status_code=422,
        content={"mensaje": "El formato del JSON es incorrecto", "detalles": exc.errors()},
    )

# 2. Endpoint para ver todos los logs desde el navegador (GET)
@app.get("/api/logs")
def obtener_logs():
    try:
        logs = list(logs_collection.find({}, {"_id": 0})) 
        return {"total_logs": len(logs), "datos": logs}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))