docker login
docker build --progress=plain -f .\linux.dockerfile -t noroozpour/gearoenix:linux-0.1 .
docker push noroozpour/gearoenix:linux-0.1