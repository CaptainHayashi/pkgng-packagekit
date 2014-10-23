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

	if (argc != 2) {
		fprintf(stderr, "usage: %s packagename\n", argv[0]);
		return 1;
	}

	db = NULL;

	err = pkg_init(NULL);
	if (err == EPKG_OK)
		err = getdb(&db);
	else
		fprintf(stderr, "could not parse config file\n");

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
		err = pkgdb_it_next(it, &match, PKG_LOAD_BASIC);
		if (err == EPKG_OK) {
			const char *name;
			const char *repo;
			const char *desc;
			const char *origin;

			pkg_get(match, PKG_NAME, &name, PKG_DESC, &desc, PKG_REPONAME, &repo, PKG_ORIGIN, &origin);

			printf("name: %s\nrepo: %s\ndesc: %s\n", name, repo, desc);
		}

	} while (err == EPKG_OK);

	pkg_free(match);
}
