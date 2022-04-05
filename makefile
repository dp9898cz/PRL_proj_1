.PHONY: doc zip
all: doc zip

clean:
	rm -rf xpatek08.pdf xpatek08.zip

doc:
	npx markdown-pdf README.md
	mv README.pdf xpatek08.pdf
zip:
	zip xpatek08.zip xpatek08.pdf test.sh oems.cpp