import unittest
from mochi.alpha.client import Client, ResourceHandle
from mochi.alpha.server import Provider
import pymargo
from pymargo.core import Engine


class TestResourceHandle(unittest.TestCase):

    def setUp(self):
        self.engine = Engine("na+sm", pymargo.core.server)
        self.provider = Provider(engine=self.engine,
                                 provider_id=42,
                                 config={
                                     "resource": {
                                         "type": "dummy"
                                     }
                                 })
        self.client = Client(engine=self.engine)

    def tearDown(self):
        del self.client
        del self.engine

    def test_create_resource_handle(self):
        handle = self.client.make_resource_handle(address=str(self.engine.address),
                                                  provider_id=42)

    def test_compute_sum(self):
        handle = self.client.make_resource_handle(address=str(self.engine.address),
                                                  provider_id=42)
        future = handle.compute_sum(34, 56)
        self.assertEqual(future.wait(), 90)

    def test_compute_sum_with_timeout(self):
        handle = self.client.make_resource_handle(address=str(self.engine.address),
                                                  provider_id=42)
        future = handle.compute_sum_with_timeout(34, 56, timeout=500)
        self.assertEqual(future.wait(), 90)

    def test_compute_sum_arrays(self):
        handle = self.client.make_resource_handle(address=str(self.engine.address),
                                                  provider_id=42)
        import array
        x = array.array('i', [1, 2, 3])
        y = array.array('i', [4, 5, 6])
        r = array.array('i', [0, 0, 0])
        future = handle.compute_sums(x, y, r)
        self.assertEqual(future.wait(), True)
        for i in range(0, 3):
            self.assertEqual(r[i], x[i] + y[i])
