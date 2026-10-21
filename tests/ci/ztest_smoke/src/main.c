#include <errno.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

K_MSGQ_DEFINE(test_queue, sizeof(uint32_t), 2, 4);

static void reset_queue(void* fixture)
{
    ARG_UNUSED(fixture);
    k_msgq_purge(&test_queue);
}

ZTEST_SUITE(ride_ci_message_queue, NULL, NULL, reset_queue, NULL, NULL);

ZTEST(ride_ci_message_queue, test_fifo_preserves_order)
{
    const uint32_t first = 17U;
    const uint32_t second = 42U;
    uint32_t received = 0U;

    zassert_ok(k_msgq_put(&test_queue, &first, K_NO_WAIT));
    zassert_ok(k_msgq_put(&test_queue, &second, K_NO_WAIT));

    zassert_ok(k_msgq_get(&test_queue, &received, K_NO_WAIT));
    zassert_equal(received, 999U); /* WP-003F temporary negative test */
    zassert_ok(k_msgq_get(&test_queue, &received, K_NO_WAIT));
    zassert_equal(received, second);
}

ZTEST(ride_ci_message_queue, test_capacity_and_empty_queue)
{
    const uint32_t value = 123U;
    uint32_t received = 0U;

    zassert_equal(k_msgq_get(&test_queue, &received, K_NO_WAIT), -ENOMSG);

    zassert_ok(k_msgq_put(&test_queue, &value, K_NO_WAIT));
    zassert_ok(k_msgq_put(&test_queue, &value, K_NO_WAIT));
    zassert_equal(k_msgq_put(&test_queue, &value, K_NO_WAIT), -ENOMSG);

    zassert_ok(k_msgq_get(&test_queue, &received, K_NO_WAIT));
    zassert_ok(k_msgq_get(&test_queue, &received, K_NO_WAIT));
    zassert_equal(k_msgq_get(&test_queue, &received, K_NO_WAIT), -ENOMSG);
}
