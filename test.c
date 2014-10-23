/* Test to make sure that pkgng and packagekit are both being linked to. */

#define I_KNOW_THE_PACKAGEKIT_GLIB2_API_IS_SUBJECT_TO_CHANGE

#include <stdio.h>
#include "pkg.h"
#include "packagekit-glib2/packagekit.h"

#include "glib-object.h"
#include "gio/gio.h"

void
network_status_changed_callback(GNetworkMonitor *nm, gboolean available, gpointer ignore)
{
	if (nm && (ignore == NULL))
		printf("network: %s\n", ((available == TRUE) ? "up" : "down"));
}

int
main(int argv, char **argc)
{
	GNetworkMonitor	       *nmon;
	gboolean		up;

	g_type_init();
	nmon = g_network_monitor_get_default();
	g_signal_connect(nmon, "network-changed", G_CALLBACK(network_status_changed_callback), NULL);

	printf("pkgng version: %s\n", PKGVERSION);
	printf("packagekit version: %i.%i.%i\n", PK_MAJOR_VERSION, PK_MINOR_VERSION, PK_MICRO_VERSION);
	printf("network: %s\n", ((g_network_monitor_get_network_available(nmon) == TRUE) ? "up" : "down"));
	
	for(;;)
		;
	return 0;
}
