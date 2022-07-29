@testset "Renormalization" begin
    # Write your tests here.
    o, c1, c2 = 1, 2, 3
    n = o * 100 + c1 * 10 + c2
    order = CompositeOrder(o, [c1, c2])
    @test order.order == o
    @test order.ct == [c1, c2]
    @test Renorm.short(order) == n
    @test Renorm.short(CompositeOrder(n)) == n
    @test Renorm.short(CompositeOrder("$n")) == n

    @test order == [o, c1, c2]
    @test order == (o, c1, c2)
    @test order != [o, c1, c2, 2]
end