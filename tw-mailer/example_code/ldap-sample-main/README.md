# LDAP Sample in C

```
ldapsearch -H ldap://ldap.technikum-wien.at -x -LLL -b "dc=technikum-wien,dc=at" -D "uid=<your user>,ou=people,dc=technikum-wien,dc=at" '(uid=if22b00*)' -W dn cn -ZZ
```