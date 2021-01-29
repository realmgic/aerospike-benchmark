
#include <transaction.h>

#include <benchmark.h>
#include <workload.h>


struct threaddata* init_tdata(clientdata* cdata, struct thr_coordinator* coord,
		uint32_t t_idx)
{
	struct threaddata* tdata =
		(struct threaddata*) cf_malloc(sizeof(struct threaddata));

	tdata->cdata = cdata;
	tdata->coord = coord;
	tdata->random = as_random_instance();
	tdata->t_idx = t_idx;
	tdata->do_work = false;
	tdata->finished = false;

	return tdata;
}


static void linear_writes(struct threaddata* tdata,
		clientdata* cdata, struct stage* stage)
{

	while (as_load_uint8((uint8_t*) &tdata->do_work)) {

	}
}


void* transaction_worker(void* udata)
{
	struct threaddata* tdata = (struct threaddata*) udata;
	clientdata* cdata = tdata->cdata;
	struct thr_coordinator* coord = tdata->coord;

	while (!as_load_uint8((uint8_t*) &tdata->finished)) {
		uint32_t stage_idx = as_load_uint32(&tdata->stage_idx);
		struct stage* stage = &cdata->stages.stages[stage_idx];
		switch (stage->workload.type) {
			case WORKLOAD_TYPE_LINEAR:
				linear_writes(tdata, cdata, stage);
				break;
			case WORKLOAD_TYPE_RANDOM:
				break;
			case WORKLOAD_TYPE_DELETE:
				break;
		}
		// check tdata->finished before locking
		if (as_load_uint8((uint8_t*) &tdata->finished)) {
			break;
		}
		thr_coordinator_wait(coord);
	}

	return NULL;
}

void* transaction_worker_async(void* udata)
{
	return NULL;
}

