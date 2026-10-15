# Trabajo-Grupal-Algoritmos
# TP Integrador — El buffet de Alberto
**Grupo N° [2]**

## Integrantes
| Nombre | Usuario de GitHub | Parte |
|---|---|---|
| [Agustin Uriel Ludi] | NVulpex | normalizacion.cpp |
| Valentino Taravini | valentaravini | normalizacion.cpp |
| Joaquin Alegre | a7egre | ventas.cpp |
| [Fidel Tomas Guaricuyu] | Tomas-crea | cierre.cpp |
| Franco Ledesma | francoledesma05 | resumen.cpp |

## Cómo compilar
```
g++ normalizacion.cpp -o normalizacion
g++ ventas.cpp -o ventas
g++ cierre.cpp -o cierre
g++ resumen.cpp -o resumen
```

## Orden de ejecución
Poner `comandas_historicas.dat` e `inventario.dat` en la misma carpeta y correr:
1. **normalizacion**: genera `mozos.dat` y las planillas `comandas_dd-mm-aaaa.dat`, y actualiza `inventario.dat`.
2. **ventas**: pide la fecha y carga ventas en la planilla de ese día (la crea si no existe).
3. **cierre**: pide número de semana, mes y fechas, y genera `comandas_semana_sX-mm.dat`.
4. **resumen**: pide el archivo semanal y muestra el reporte por pantalla.

## Cómo resolvimos las partes 
- **Mozos:** se recorre el histórico y se busca cada nombre en un vector (búsqueda secuencial). Si no está, se agrega con un id nuevo. Si ya está, se le suma la comisión.
- **Planillas por día:** cada venta se agrega a la planilla de su fecha. Al final, cada planilla se carga en memoria, se ordena por idMozo (burbuja) y se reescribe. Antes de empezar se borran las planillas viejas para no duplicar ventas.
- **Stock:** el inventario está ordenado por código pero tiene huecos, así que usamos **búsqueda binaria con fseek** y actualizamos el registro en el lugar, sin rehacer el archivo.
- **Claves:** a cada carácter se le suma K = 7 (corrimiento). La clave inicial es el número del mozo. Para verificar, a la clave que se tipea se le suma 7 y se compara con la guardada. Se puede deshacer restando 7.
- **Ventas:** primero se valida que el mozo exista, después la clave. El precio se busca con búsqueda binaria y la comisión es el 10% de lo vendido. Al terminar, la planilla del día se ordena por mozo.
- **Cierre:** las planillas diarias ya están ordenadas por mozo, así que se combinan con **apareo**: siempre se toma la venta con el idMozo menor.
- **Resumen:** **corte de control** por idMozo sobre la planilla semanal, que ya viene ordenada por mozo desde cierre. Se recorre el archivo una sola vez, sin cargarlo en memoria, con lectura anticipada y `while (fread(...) == 1)`. Por cada mozo muestra la cantidad de productos y la comisión, y al final el total de productos del buffet. El nombre se busca en `mozos.dat` por idMozo. Casos raros: el último mozo se cierra cuando `fread` falla (no necesita un mozo siguiente), si el archivo está vacío avisa que no hay registros, si no se puede abrir muestra un error, y si el id no está en `mozos.dat` muestra "Desconocido".
