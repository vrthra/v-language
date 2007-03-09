all:
	javac -Xlint:unchecked -d pkg v/*.java
	(cd pkg && jar -cmf v.mf v.jar v)

run:
	(cd pkg && java -jar v.jar)
test:
	cd pkg && java -jar v.jar ../v/examples.v

