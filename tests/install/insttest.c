#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pkg.h"

void handle_it(struct pkgdb_it *it);
int getdb(struct pkgdb **db);

int
getdb(struct pkgdb **db)
{
	int err;

	 err = pkgdb_access(PKGDB_MODE_READ, PKGDB_DB_REPO);

	if (err == EPKG_ENOACCESS)
		fprintf(stderr, "check your privilege: %i\n", err);
	else if (err != EPKG_OK)
		fprintf(stderr, "access error: %i\n", err);
	else {
		err = pkgdb_open(db, PKGDB_REMOTE);
		if (err != EPKG_OK)
			fprintf(stderr, "open error: %i\n", err);
	}
	return err;
}

int
main(int argc, char **argv)
{
	struct pkg *pkg;
	struct pkgdb *db;
	struct pkg_jobs *jobs;
	int err;

	db = NULL;
	pkg = NULL;
	jobs = NULL;

	err = ((argc == 2 || argc == 3) ? EPKG_OK : EPKG_FATAL);
	if (err == EPKG_OK) {
		err = pkg_init(NULL);
		if (err != EPKG_OK)
			fprintf(stderr, "could not parse config file\n");
	} else
		fprintf(stderr, "usage: %s name [repo]\n", argv[0]);

	if (err == EPKG_OK)
		err = getdb(&db);

	if (err == EPKG_OK) {
		err = pkg_jobs_new(&jobs, PKG_JOBS_INSTALL, db);
		if (err != EPKG_OK)
			fprintf(stderr, "couldn't create jobs\n");
	}

	if (err == EPKG_OK && argc == 3) {
		char *rep;

		/* Test what happens to set_repository if it's passed NULL */
		if (strcmp(argv[2], "null") == 0) {
			rep = NULL;
			printf("using null\n");
		} else
			rep = argv[2];

		err = pkg_jobs_set_repository(jobs, rep);
		if (err != EPKG_OK)
			fprintf(stderr, "couldn't set repo\n");
	}

	if (err == EPKG_OK) {
		err = pkg_jobs_add(jobs, MATCH_EXACT, argv + 1, 1);
		if (err != EPKG_OK)
			fprintf(stderr, "couldn't add to job\n");
	}

	if (err == EPKG_OK) {
		err = pkg_jobs_solve(jobs);
		if (err != EPKG_OK)
			fprintf(stderr, "couldn't solve: %u\n", err);
	}

	if (err == EPKG_OK) {
		int c;

		c = pkg_jobs_count(jobs);
		if (c > 0) {
			printf("%i jobs\n", c);
			while (pkg_jobs(jobs, &pkg) == EPKG_OK)
				pkg_printf("%n\n", pkg);
		} else
			fprintf(stderr, "nothing to do\n");
	}

	printf("shutting down\n");
	pkg_jobs_free(jobs);
	pkg_free(pkg);
	pkgdb_close(db);
	pkg_shutdown();
	printf("shutdown\n");
}
