

#include <assert.h>		/* assert */
#include <glib.h>		/* g_free, gchar */
#include "pkg.h"		/* struct pkg, pkg_get */

#include "emit.h"		/* emit_... */
#include "pkgutils.h"		/* pkgutils_... */

/* Functions for emitting a package to the PackageKit daemon. */

/*
 * Emits a package through the backend with the given info enum.
 */
void
emit_package(struct pkg *pkg, PkBackend *backend, PkInfoEnum info)
{
	char           *comment;
	gchar          *id;

	assert(pkg != NULL);
	assert(backend != NULL);

	comment = id = NULL;

	pkg_get(pkg, PKG_COMMENT, &comment);
	id = pkgutils_pkg_to_id(pkg);
	(void)pk_backend_package(backend, info, id, comment);
	g_free(id);
}

/*
 * Emits a package if it satisfies the given filter set.
 */
void
emit_filtered_package(struct pkg *pkg, PkBackend *backend, PkBitfield filters,
    PkInfoEnum info)
{

	assert(pkg != NULL);
	assert(backend != NULL);

	if (pkgutils_pkg_matches_filters(pkg, filters)) {
		emit_package(pkg, backend, info);
	}
}
