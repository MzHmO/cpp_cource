# В другом терминале
docker exec signal_container cat /home/box/pid

# Получим PID процесса

docker exec signal_container kill -TERM <PID>
# Процесс не умрёт

docker exec signal_container kill -INT <PID>  
# Процесс не умрёт

docker exec signal_container kill -KILL <PID>
# Убьёт 