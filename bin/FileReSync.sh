#!/bin/bash


#execute FileSync
./FileSync


#sync to meilisearch
#delete old doc
curl -X DELETE 'http://142.171.239.198:7700/indexes/books'

#add new doc
curl -X POST 'http://142.171.239.198:7700/indexes/books/documents?primaryKey=id' -H 'Content-Type: application/json' -H 'Authorization: Bearer aSampleMasterKey' --data-binary @books.json



