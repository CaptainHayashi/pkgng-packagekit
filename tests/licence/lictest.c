#include <stdio.h>
#include <stdlib.h>
#include "pkg.h"

void handle_it(struct pkgdb_it *it);
int getdb(struct pkgdb **db);

int
getdb(struct pkgdb **db)
{
	int err;

	/* err = pkgdb_access(PKGDB_MODE_READ, PKGDB_DB_REPO);

	if (err == EPKG_ENOACCESS)
		fprintf(stderr, "check your privilege\n", err);
	else if (err != EPKG_OK)
		fprintf(stderr, "access error: %i\n", err);
	else */{
		err = pkgdb_open(db, PKGDB_REMOTE);
		if (err != EPKG_OK)
			fprintf(stderr, "open error: %i\n", err);
	}
	return err;
}

int
main(int argc, char **argv)
{
	struct pkgdb *db;
	int err;

	db = NULL;

	err = (argc == 2 ? EPKG_OK : EPKG_FATAL);
	if (err == EPKG_OK) {
		err = pkg_init(NULL);
		if (err != EPKG_OK)
			fprintf(stderr, "could not parse config file\n");
	} else
		fprintf(stderr, "usage: %s name\n", argv[0]);

	if (err == EPKG_OK)
		err = getdb(&db);

	if (err == EPKG_OK) {
		struct pkgdb_it *it;

		it = pkgdb_query(db, argv[1], MATCH_EXACT);
		if (it == NULL)
			printf("no local matches!\n");
		else handle_it(it);

		it = pkgdb_rquery(db, argv[1], MATCH_EXACT, NULL);
		if (it == NULL)
			printf("no remote matches!\n");
		else handle_it(it);

		printf("shutting down\n");
		pkgdb_it_free(it);
		pkgdb_close(db);
		pkg_shutdown();
		printf("shutdown\n");
	}
}


void
handle_it(struct pkgdb_it *it)
{
	int err;
	struct pkg *match;

	match = NULL;
	do {
		err = pkgdb_it_next(it, &match, PKG_LOAD_BASIC | PKG_LOAD_LICENSES);
		if (err == EPKG_OK) {
			const char *name;
			struct pkg_license *lic;
			int err2;

			lic = NULL;

			pkg_get(match, PKG_NAME, &name);
			printf("name: %s\n", name);

			do {
				err2 = pkg_licenses(match, &lic);
				if (err2 == EPKG_OK)
					printf("  licence: %s\n", pkg_license_name(lic));
			} while (err2 == EPKG_OK);
		}

	} while (err == EPKG_OK);

	pkg_free(match);
}
